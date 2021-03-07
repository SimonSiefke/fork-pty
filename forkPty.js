const bindings = require('bindings')
const assert = require('assert')
const addon = bindings('forkPty.node')

const forkPtyAndExecvp = (file, argv, onExit) => {
  assert(typeof file === 'string')
  assert(Array.isArray(argv) && argv.every((arg) => typeof arg === 'string'))
  assert(typeof onExit === 'function')
  const onExit2 = () => {
    onExit()
  }
  const result = addon.forkPtyAndExecvp(file, argv, onExit2)
  return result
}

exports.forkPtyAndExecvp = forkPtyAndExecvp
