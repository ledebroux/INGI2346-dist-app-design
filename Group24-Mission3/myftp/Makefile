CLIENT = myftp
SERVER = myftpd

SOURCES_CLNT.c = utils.c myftp.c
SOURCES_CLNT.h =
SOURCES_SVC.c = shared.c
SOURCES_SVC.h =
SOURCES.x = rpspec.x

TARGETS_SVC.c = rpspec_svc.c myftpd.c rpspec_xdr.c
TARGETS_CLNT.c = rpspec_clnt.c rpspec_xdr.c
TARGETS = rpspec_xdr.c rpspec_clnt.c rpspec_svc.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)

CFLAGS += -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c99 -g -O0

LDLIBS = -lnsl

RPCGENFLAGS = -CN

all : $(CLIENT) $(SERVER)

$(TARGETS) : $(SOURCES.x)
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c)

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c)

$(CLIENT) : $(OBJECTS_CLNT)
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS)

$(SERVER) : $(OBJECTS_SVC)
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

clean:
	$(RM) core $(TARGETS) $(OBJECTS_CLNT) $(OBJECTS_SVC) $(CLIENT) $(SERVER)
