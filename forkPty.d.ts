import type { Socket } from 'net'

export const forkPtyAndExecvp: (
  file: string,
  argv: readonly string[]
) => {
  readonly fd: number
  readonly pid: number
  readonly socket: Socket
}
