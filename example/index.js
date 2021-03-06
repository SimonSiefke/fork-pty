import { forkPtyAndExecvp } from '../forkPty.js'
import { ReadStream } from 'tty'

const { fd } = forkPtyAndExecvp('ls', ['ls', '-l'], (x) => {
  console.log('exit callback', x)
})

const readStream = new ReadStream(fd)

readStream.on('data', (data) => {
  console.log({ data: data.toString() })
})

readStream.on('error', (error) => {
  if (error.code === 'EIO') {
    return
  }
  throw error
})

// setTimeout(() => {}, 1000)
// TODO kill process when done
