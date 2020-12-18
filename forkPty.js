const bindings = require('bindings')
const assert = require('assert')
const addon = bindings('forkPty.node')

const forkPtyAndExeclp = (file, ...argv) => {
  assert(typeof file === 'string', 'file must be of type string')
  assert(
    argv.every((arg) => typeof arg === 'string'),
    'argv must be of type string[]',
  )
  const fileDescriptor = addon.forkPtyAndExeclp(file, ...argv)
  return fileDescriptor
}

exports.forkPtyAndExeclp = forkPtyAndExeclp

const forkPtyAndExecvp = (file, argv) => {
  assert(typeof file === 'string')
  assert(Array.isArray(argv) && argv.every((arg) => typeof arg === 'string'))
  const fileDescriptor = addon.forkPtyAndExecvp(file, argv)
  return fileDescriptor
}

exports.forkPtyAndExecvp = forkPtyAndExecvp
