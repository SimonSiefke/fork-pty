import { forkPtyAndExecvp } from './forkPty.js'

test('spawn bash', () => {
  const { fd, ptySocket } = forkPtyAndExecvp('bash', ['bash', '-i'])
  expect(fd).toBeGreaterThan(0)
  ptySocket.destroy()
})

test('spawn ls', async () => {
  const { ptySocket } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const data = await new Promise((resolve) => {
    ptySocket.on('data', (data) => {
      ptySocket.destroy()
      resolve(data.toString())
    })
  })

  expect(data).toMatchInlineSnapshot(`
    "workflows
    "
  `)
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
