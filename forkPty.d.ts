import type { Socket } from 'net'

export const forkPtyAndExecvp: (
  file: string,
  argv: readonly string[]
) => {
  readonly fd: number
  readonly pid: number
}

export class PipeSocket extends Socket {
  constructor(fd: number) {}
}
