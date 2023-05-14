import bindings from 'bindings'
import assert from 'node:assert'
import { Socket } from 'node:net'

const addon = bindings('forkPty.node')

export class PipeSocket extends Socket {
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
  return {
    fd,
    pid,
  }
}
