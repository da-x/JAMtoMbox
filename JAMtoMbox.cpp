#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <cstddef>
#include <codecvt>
#include <string>
#include <locale>
#include <iconv.h>
#include <string.h>

#include "jammb.h"

struct JAMWithMetadata {
    std::vector<uint32_t> message_ids;
    JAMAPIREC jam;
};

static std::string convert_to_utf8(const std::string &text, const std::string &encoding)
{
    std::vector<char> output;
    for (auto &i: text) {
        // Some basic Fidonet control code conversion
        if (i == 13) {
            output.push_back('\n');
        } else if ((unsigned char)i == 0xff) {
            output.push_back(' ');
        } else {
            output.push_back(i);
        }
    }
    std::string textvec(output.begin(), output.end());

    iconv_t icd = iconv_open("UTF-8", encoding.c_str());
    char *inbuf = &output[0];
    size_t in_len = output.size();
    size_t out_len = in_len * 8;
    char *origbuf = (char *)malloc(out_len);
    char *outbuf = origbuf;
    ssize_t converted = iconv(icd, &inbuf, &in_len, &outbuf, &out_len);
    iconv_close(icd);
    assert(converted != -1);
    std::string str(origbuf, outbuf - origbuf);
    free(origbuf);

    return str;
}

static void convert_current_message(JAMAPIREC &jam, const std::string &encoding)
{
    printf("\n");
    printf("From e21f66baa0c8d9f036feaff42411f32315dfb71b Mon Sep 17 00:00:00 2001\n");

    uint32_t pos = JAMsysAlign(0);
    JAMSUBFIELDptr SubFieldPtr;

    std::string from_name;
    std::string from_address;
    std::string to_name;
    std::string to_address;
    std::string subject;
    std::string mailer;
    std::string list_id;

    while (pos < jam.Hdr.SubfieldLen) {
        SubFieldPtr = (JAMSUBFIELD *)JAMsysAddPtr(jam.WorkBuf, pos);

        std::string str(SubFieldPtr->Buffer, SubFieldPtr->DatLen);
        switch (SubFieldPtr->LoID) {
        case JAMSFLD_SENDERNAME:
            from_name = str;
            break;
        case JAMSFLD_RECVRNAME:
            to_name = str;
            break;
        case JAMSFLD_MSGID:
            from_address = str;
            break;
        case JAMSFLD_REPLYID:
            to_address = str;
            break;
        case JAMSFLD_SUBJECT:
            subject = str;
            break;
        case JAMSFLD_PID:
            mailer = str;
            break;
        case JAMSFLD_FTSKLUDGE:
            break;
        case JAMSFLD_PATH2D:
            list_id = str;
            break;
        default:
            printf("Header-%d: %s\n", SubFieldPtr->LoID, str.c_str());
            break;
        }

        pos += JAMsysAlign(SubFieldPtr->DatLen + (UINT32)sizeof(JAMSUBFIELD));
    }

    printf("From: %s <%s>\n", from_name.c_str(), convert_to_utf8(from_address, encoding).c_str());
    time_t date_written = jam.Hdr.DateWritten;
    char date_written_str[0x100] = {};
    printf("To: %s <%s>\n", to_name.c_str(), convert_to_utf8(to_address, encoding).c_str());
    printf("Date: %s", ctime_r(&date_written, date_written_str));
    printf("Subject: %s\n", convert_to_utf8(subject, encoding).c_str());
    printf("X-Mailer: %s\n", mailer.c_str());

    int r = JAMmbFetchMsgTxt(&jam, 1);
    assert(r == 1);
    std::string text(jam.WorkBuf, jam.WorkPos);

    printf("%s\n", convert_to_utf8(text, encoding).c_str());
}

int main(int argc, char *argv[])
{
    JAMWithMetadata extra;
    JAMAPIREC &jam = extra.jam;
    int r;

    if (argc < 3) {
        std::cerr << "arguments: [basename] [encoding]" <<  std::endl;
        return -1;
    }

    std::string basename = argv[1];
    std::string source_encoding = argv[2];

    std::cout << std::endl;

    r = JAMsysInitApiRec(&jam, argv[1], 0x1000000);
    assert(r == 1);
    r = JAMmbOpen(&jam);
    assert(r == 1);

    JAMmbScanForMsgHdr(&jam, jam.HdrInfo.BaseMsgNum, 1, [] (JAMAPIRECptr ptr) {
        JAMWithMetadata *const extra = (JAMWithMetadata *)((char *)ptr - offsetof(JAMWithMetadata, jam));
        extra->message_ids.push_back(ptr->LastMsgNum);
        return ScanMsgHdrNextHdr;
    });

    for (auto &id: extra.message_ids) {
        r = JAMmbFetchMsgHdr(&jam, id, 1);
        if (r != 1) {
            continue;
        }

        convert_current_message(jam, source_encoding);
    }

    r = JAMmbClose(&jam);
    assert(r == 1);
	r = JAMsysDeinitApiRec(&jam);
    assert(r == 1);

    return 0;
}
