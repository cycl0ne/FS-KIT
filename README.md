FS-KIT
======

This is the Test Branch. Here i test an own implementation of a filesystem.

The idea behind htis filesystem: Every block has an 16byte header, so on a crash, the whole filesystem should be able to refind all data on disk. (theory! Still in development).

Also the journaling is missing :-( and a lot of warnings on compile.


------------------------------------------------------------
FS-Kit from the Book: "Practical File System Design with the Be File System"
The book can be downloaded at: http://www.nobius.org/~dbg/practical-file-system-design.pdf

For the Copyright of the Code:

  THIS CODE COPYRIGHT DOMINIC GIAMPAOLO.  NO WARRANTY IS EXPRESSED
  OR IMPLIED.  YOU MAY USE THIS CODE AND FREELY DISTRIBUTE IT FOR
  NON-COMMERCIAL USE AS LONG AS THIS NOTICE REMAINS ATTACHED.

  FOR COMMERCIAL USE, CONTACT DOMINIC GIAMPAOLO (dbg@be.com).

  Dominic Giampaolo
  2523 Nedson Ct.
  Mountain View, CA. 94043
  dbg@be.com


