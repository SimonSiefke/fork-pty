const bindings = require('bindings')
const assert = require('assert')
const addon = bindings('forkPty.node')

const forkPtyAndExecvp = (file, argv) => {
  assert(typeof file === 'string')
  assert(Array.isArray(argv) && argv.every((arg) => typeof arg === 'string'))
  const fileDescriptor = addon.forkPtyAndExecvp(file, argv)
  return fileDescriptor
}

exports.forkPtyAndExecvp = forkPtyAndExecvp
