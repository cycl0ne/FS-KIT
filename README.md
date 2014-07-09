FS-KIT
======

In "original" you will find the original source, here in "master" you will find a "bugfixed" version from me.

The first problem with the code is, that it had been designed for a 32bit architecture. Thus, when you compile it under Linux 64-Bit, the inode structure will break the ^2 Limit, since pointer and padding are 8byte (128bytes for inode will explode to 240bytes).
Executing the FSH / makefs will go into an error then. Will try to fix this.

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


