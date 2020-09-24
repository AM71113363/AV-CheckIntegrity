build.bat will work only with Compiler:  Dev-C++ 4.9.9.*


run AV.exe (32Bit Windows) 

1.Drag-Drop your file(EXE,dll,ecc...)
2.Follow the instructions.

How it Works:
1.Calculate the hash MD5 of the file.
2.Rename the file with the first 4 characters of HASH-MD5
sample-> from  \myfile.exe         
               hash-MD5 = 1A07BCDEF...
         to    \1A07-myfile.exe  
         
IMPORTANT:
Don't RENAME the first 5 characters of the file,otherwise it will not work


PS: Some viruses after infecting your PC will try to infect other files(EXE,dll,...).
    Some low-cost USB storages can corrupt your files due to damaged memory cells.
    
