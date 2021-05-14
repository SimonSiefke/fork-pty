import { forkPtyAndExecvp } from 'fork-pty'

const { socket } = forkPtyAndExecvp('bash', ['bash', '-i'])

socket.on('data', (data) => {
  console.log({ data: data.toString() })
})

// TODO kill process when done
