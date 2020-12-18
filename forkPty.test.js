const {
  forkPtyAndExeclp,
  forkPtyAndExecvp,
  forkPtyAndExeclpe,
} = require('./forkPty.js')

test('forkPtyAndExeclp', () => {
  const fileDescriptor = forkPtyAndExeclp('bash', '-i')
  expect(fileDescriptor).toBeGreaterThan(0)
})

test('forkPtyAndExeclpe', () => {
  const fileDescriptor = forkPtyAndExeclpe('bash', '-i')
  expect(fileDescriptor).toBeGreaterThan(0)
})

test('forkPtyAndExecvp', () => {
  const fileDescriptor = forkPtyAndExecvp('bash', ['-i'])
  expect(fileDescriptor).toBeGreaterThan(0)
})
