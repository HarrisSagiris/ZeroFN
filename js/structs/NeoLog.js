module.exports = {

  Log(data) {
    console.log(`[\x1b[35;1mZeroFN\x1b[0m]: ${data}\x1b[0m`)
  },
  PreLoad(data) {
    console.log(`[\x1b[36mZeroFN\x1b[0m]: ${data}\x1b[0m`)
  },
  Error(data) {
    console.log(`[\x1b[31mZeroError\x1b[0m]: ${data}\x1b[0m`)
  },
  Debug(data) {
    console.log(`[\x1b[32mDEBUG\x1b[0m]: ${data}\x1b[0m`)
  },
  DB(data) {
    console.log(`[\x1b[33mMONGO\x1b[0m]: ${data}\x1b[0m`)
  }
}