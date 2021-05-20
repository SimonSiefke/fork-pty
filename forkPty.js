import bindings from 'bindings'
import assert from 'assert'
import { Socket } from 'net'

const addon = bindings('forkPty.node')

class PipeSocket extends Socket {
  constructor(fd) {
    // @ts-ignore
    const pipeWrap = process.binding('pipe_wrap')
    const handle = new pipeWrap.Pipe(pipeWrap.constants.SOCKET)
    handle.open(fd)
    // @ts-ignore
    super({ handle })
  }
}

export const forkPtyAndExecvp = (file, argv) => {
  assert(typeof file === 'string')
  assert(Array.isArray(argv) && argv.every((arg) => typeof arg === 'string'))
  const { fd, pid } = addon.forkPtyAndExecvp(file, argv)
  const ptySocket = new PipeSocket(fd)
  return {
    fd,
    pid,
    ptySocket,
  }
}
