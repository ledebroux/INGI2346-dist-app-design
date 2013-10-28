MYFTP : rpcgen myftp myftpd

rpcgen : rpspec.x
	rpcgen rpspec.x

myftp : myftp.c utils.c header.h rpspec_svc.c rpspec_xdr.c
	gcc utils.c myftp.c rpspec_clnt.c rpspec_xdr.c -o myftp -lnsl

myftpd : myftpd.c utils.c header.h
	gcc utils.c myftpd.c rpspec_svc.c rpspec_xdr.c -o myftpd -lnsl
