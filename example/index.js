import { forkPtyAndExecvp } from 'fork-pty'
import { ReadStream } from 'tty'

const fd = forkPtyAndExecvp('bash', ['bash', '-c', 'echo hi'])

const readStream = new ReadStream(fd)

readStream.on('data', (data) => {
  console.log({ data: data.toString() })
})
