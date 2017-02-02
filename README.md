## JAMToMbox

This utility provides rudimentary conversion from the 
[JAM Message Base Format](https://en.wikipedia.org/wiki/JAM_Message_Base_Format) to the [Mbox](https://en.wikipedia.org/wiki/Mbox) format.

### Building

Build on Linux using:

    cmake . && make

### Using

Provided a JAM archive:

    $ ls -1 FE627FC8.*

    FE627FC8.jdt
    FE627FC8.jdx
    FE627FC8.jhr
    FE627FC8.jlr

It can be converted using JAMToMbox in the following fashion, assuming a known
encoding of the text. The output encoding is UTF-8. For example

    $ JAMToMbox FE627FC8 CP862

## Limitations

The program tries to do its best to convert the per-message JAM subfields to
standard email header format (`<Header>: <Value>`). Currently the support for such
conversion is limited, and the resulting email address may be invalid. It is
possible to impose some kind of scheme to make it behave better, so that the
mails can be imported into IMAP account or used with modern email clients.
For now it is rudimentary.
