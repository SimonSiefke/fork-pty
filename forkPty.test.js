const { forkPtyAndExeclp } = require('./forkPty.js')

test('forkPty', () => {
  const fileDescriptor = forkPtyAndExeclp('bash', '-i')
  expect(fileDescriptor).toBeGreaterThan(0)
})
