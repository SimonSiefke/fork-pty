import { forkPtyAndExecvp } from '../forkPty.js'
import { ReadStream } from 'tty'

const { fd } = forkPtyAndExecvp('ls', ['ls', '-l'], (code) => {
  console.log('exit callback', code)
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

// for (let i = 0; i < 10; i++) {
//   createBash()
// }

// setTimeout(() => {}, 1000)
// TODO kill process when done
