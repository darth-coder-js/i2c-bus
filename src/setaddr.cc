#include <errno.h>
#include <node.h>
#include <nan.h>
#include "./i2c-dev.h"
#include "./setaddr.h"

static int SetAddr(int fd, int addr) {
  return ioctl(fd, I2C_SLAVE, addr);
}

class SetAddrWorker : public NanAsyncWorker {
public:
  SetAddrWorker(NanCallback *callback, int fd, int addr)
    : NanAsyncWorker(callback), fd(fd), addr(addr) {}
  ~SetAddrWorker() {}

  void Execute() {
    if (SetAddr(fd, addr) == -1) {
      SetErrorMessage(strerror(errno));
    }
  }

  void HandleOKCallback() {
    NanScope();

    v8::Local<v8::Value> argv[] = {
      NanNull()
    };

    callback->Call(1, argv);
  }

private:
  int fd;
  int addr;
};

NAN_METHOD(SetAddrAsync) {
  NanScope();

  int fd = args[0]->Uint32Value();
  int addr = args[1]->Uint32Value();
  NanCallback *callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new SetAddrWorker(callback, fd, addr));
  NanReturnUndefined();
}

NAN_METHOD(SetAddrSync) {
  NanScope();

  int fd = args[0]->Uint32Value();
  int addr = args[1]->Uint32Value();

  if (SetAddr(fd, addr) != 0) {
    return NanThrowError(strerror(errno), errno);
  }

  NanReturnUndefined();
}

