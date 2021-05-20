import { forkPtyAndExecvp } from 'fork-pty'

const { ptySocket } = forkPtyAndExecvp('bash', ['bash', '-i'])

ptySocket.on('data', (data) => {
  console.log({ data: data.toString() })
})

// TODO kill process when done
