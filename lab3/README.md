# Lab 2

Gold Team
-----------

Chris Opperwall
Andrew Sorensen
Eric Tran
Nick Verbos
Scott Vanderlind

Work Breakdown
---------------

Chris Opperwall
* newPage
* getLRUPage
* touchPage
* testHarness (WIP, disregarding finishing touches due to better spec in Lab 3)

Andrew Sorensen
* pageExistsInBuffer
* getBufferIndex

Eric Tran
* checkpoint
* pageDump
* printPage
* printBlock

Nick Verbos
* readPage
* writePage
* flushPage
* commence

Scott Vanderlind
* pinPage
* unPinPage
* squash


__To Build__

Run `make TFS468` to get the object files for TinyFS.
Run `make harness` to build buffer manager and the test harness.

__To Run__

./harness < [test_file]
