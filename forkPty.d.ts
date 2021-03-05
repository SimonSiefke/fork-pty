export const forkPtyAndExecvp: (
  file: string,
  argv: readonly string[]
) => {
  readonly fd: number
  readonly pid: number
}
