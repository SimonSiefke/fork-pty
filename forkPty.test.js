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
  const { fd, ptySocket } = forkPtyAndExecvp('bash', ['bash', '-i'])
  expect(fd).toBeGreaterThan(0)
  ptySocket.destroy()
})

test('spawn echo', async () => {
  const { fd, ptySocket } = forkPtyAndExecvp('echo', [
    'echo',
    'Immediate output',
  ])
  expect(fd).toBeGreaterThan(0)
  const data = await waitForData(ptySocket)
  expect(data).toBe(`Immediate output\r\n`)
})

test('spawn ls', async () => {
  const { ptySocket } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const data = await waitForData(ptySocket)
  expect(data).toBe(`workflows\r\n`)
})

test('spawn multiple pseudo terminals', async () => {
  const chars = ['a', 'b', 'c']
  const sockets = chars
    .map((char) => forkPtyAndExecvp('echo', ['echo', char]))
    .map((result) => result.ptySocket)
  const data = await Promise.all(sockets.map(waitForData))
  expect(data).toEqual(['a\r\n', 'b\r\n', 'c\r\n'])
})

// TODO
test.skip('valid input', (done) => {
  const { ptySocket } = forkPtyAndExecvp('ls', ['ls', '.github'])
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
  const { fd, ptySocket } = forkPtyAndExecvp('non-existent-command', [])
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
