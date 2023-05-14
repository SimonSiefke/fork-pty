import { ReadStream } from 'node:tty'
import { forkPtyAndExecvp } from './forkPty.js'

const waitForData = async (ptySocket) => {
  const data = await new Promise((resolve) => {
    ptySocket.on('data', (data) => {
      ptySocket.destroy()
      resolve(data.toString())
    })
  })
  return data
}

test('spawn bash', () => {
  const { fd } = forkPtyAndExecvp('bash', ['bash', '-i'])
  const ptySocket = new ReadStream(fd)
  expect(fd).toBeGreaterThan(0)
  ptySocket.destroy()
})

test('spawn echo', async () => {
  const { fd } = forkPtyAndExecvp('echo', ['echo', 'Immediate output'])
  expect(fd).toBeGreaterThan(0)
  const ptySocket = new ReadStream(fd)
  const data = await waitForData(ptySocket)
  expect(data).toBe(`Immediate output\r\n`)
})

test('spawn ls', async () => {
  const { fd } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const ptySocket = new ReadStream(fd)
  const data = await waitForData(ptySocket)
  expect(data).toBe(`workflows\r\n`)
})

test('spawn multiple pseudo terminals', async () => {
  const chars = ['a', 'b', 'c']
  const sockets = chars
    .map((char) => forkPtyAndExecvp('echo', ['echo', char]))
    .map((result) => result.fd)
    .map((fd) => new ReadStream(fd))
  const data = await Promise.all(sockets.map(waitForData))
  expect(data).toEqual(['a\r\n', 'b\r\n', 'c\r\n'])
})

// TODO
test.skip('valid input', (done) => {
  const { fd } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const ptySocket = new ReadStream(fd)
  ptySocket.on('data', (data) => {
    console.log({ data: data.toString() })
  })
  ptySocket.on('error', (error) => {
    // @ts-ignore
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  ptySocket.on('close', (had_error) => {
    expect(had_error).toBe(false)
    done()
  })
})

test.skip('invalid input', (done) => {
  const { fd } = forkPtyAndExecvp('non-existent-command', [])
  const ptySocket = new ReadStream(fd)
  ptySocket.on('data', () => {})
  ptySocket.on('error', (error) => {
    // @ts-ignore
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  ptySocket.on('close', (had_error) => {
    expect(had_error).toBe(true)
    done()
  })
})
