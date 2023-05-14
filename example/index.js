import { forkPtyAndExecvp } from '../forkPty.js'
import { ReadStream } from 'node:tty'

const createBash = () => {
  const { fd, pid } = forkPtyAndExecvp('bash', ['bash', '-i'], (x) => {
    console.log('exit callback', x)
  })
  console.log({ pid })
  console.log({ fd })
  const readStream = new ReadStream(fd)

  readStream.on('data', (data) => {
    console.log({ data: data.toString() })
  })

  readStream.on('close', () => {
    console.log('read closed')
  })
  readStream.on('connect', () => {
    console.log('read connected')
  })
  readStream.on('error', (error) => {
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  setTimeout(() => {
    console.log('kill child pid')
    process.kill(pid, 'SIGHUP')
    // readStream.destroy()
  }, 1000)
}

// for (let i = 0; i < 1; i++) {
createBash()
// }

// setTimeout(() => {}, 1000)
// TODO kill process when done
