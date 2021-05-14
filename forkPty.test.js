const { forkPtyAndExecvp } = require('./forkPty.js')

test('spawn bash', () => {
  const { fd } = forkPtyAndExecvp('bash', ['bash', '-i'])
  expect(fd).toBeGreaterThan(0)
})

test('spawn ls', async () => {
  const { socket } = forkPtyAndExecvp('ls', ['ls', '.github'])
  const data = await new Promise((resolve) => {
    socket.on('data', (data) => {
      socket.destroy()
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
  const { socket } = forkPtyAndExecvp('ls', ['ls', '.github'])
  socket.on('data', (data) => {
    console.log({ data: data.toString() })
  })
  socket.on('error', (error) => {
    // @ts-ignore
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  socket.on('close', (had_error) => {
    expect(had_error).toBe(false)
    done()
  })
})

test.skip('invalid input', (done) => {
  const { fd, socket } = forkPtyAndExecvp('non-existent-command', [])
  socket.on('data', () => {})
  socket.on('error', (error) => {
    // @ts-ignore
    if (error.code === 'EIO') {
      return
    }
    throw error
  })
  socket.on('close', (had_error) => {
    expect(had_error).toBe(true)
    done()
  })
})
