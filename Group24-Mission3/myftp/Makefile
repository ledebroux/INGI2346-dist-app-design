MYFTP : myftp myftpd

myftp : myftp.c utils.c rpspec_clnt.c rpspec_xdr.c utils.h
	gcc utils.c myftp.c rpspec_clnt.c rpspec_xdr.c -o myftp -lnsl

myftpd : myftpd.c utils.c rpspec_svc.c rpspec_xdr.c utils.h
	gcc utils.c myftpd.c rpspec_svc.c rpspec_xdr.c -o myftpd -lnsl
