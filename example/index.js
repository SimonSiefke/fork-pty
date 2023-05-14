import { forkPtyAndExecvp } from 'fork-pty'
import { ReadStream } from 'node:tty'

const { fd } = forkPtyAndExecvp('bash', ['bash', '-i'])

const ptySocket = new ReadStream(fd)

ptySocket.on('data', (data) => {
  console.log({ data: data.toString() })
})

// TODO kill process when done
