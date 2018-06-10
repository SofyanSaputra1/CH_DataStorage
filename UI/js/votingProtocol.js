document.addEventListener("scatterLoaded", function(extension) {
  // Optional configuration..
  config = {
    chainId: "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f", // 32 byte (64 char) hex string
    keyProvider: ["5JuQqkfhfc32A7t2tEZAhG1dGAz5tQjkgkge6Bgv7H3QLj5eGHg"], // WIF string or array of keys..
    httpEndpoint: "http://127.0.0.1:8888",
    expireInSeconds: 60,
    broadcast: true,
    debug: false, // API and transactions
    sign: true,
  }

  const eos = Eos(config)

  const contract = eos.contract("testaccount").then(contract => {

  // return contract.vote("ch", "ch", "10", { authorization: "testaccount@active" })
  })
  .then(() => console.log("VOTE FINISHED"))
  .then(() => {
    return eos
          .getTableRows({
            json: true,
            code: "testaccount",
            scope: "testaccount",
            table: "mpvotes",
            //table_key: "",
            // lower_bound: null,
            // upper_bound: null,
            // limit: 5,
          })
  })
  .then(rows => {
    console.log("ROWS", rows)
  })
})

// document.addEventListener("scatterLoaded", function(extension) {
//   const scatter = window.scatter
//   const eos = scatter.eos(networkConfig, Eos)
//   scatter.suggestNetwork(networkConfig).then(() =>
//     scatter
//       .forgetIdentity()
//       .then(() =>
//         scatter.getIdentity({
//           accounts: [
//             {
//               blockchain: "eos",
//               host: "127.0.0.1",
//               port: 8888,
//               chainId: "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f",
//             },
//           ],
//         })
//       )
//       .then(identity => {
//         const contract = eos.contract("ch2")
//         console.log(identity)
//         return contract.then(contract => ({ identity, contract }))
//       })
//       .then(({ contract, identity }) => {
//         const entry = {
//           data_hash: "",
//           mp: identity.accounts[0].name,
//           data_type: "",
//           deltaToken: 10,
//           timestamp: new Date().toISOString(),
//         }

//         return eos
//           .getTableRows({
//             json: true,
//             code: "ch2",
//             scope: "owner",
//             table: "researchform",
//             table_key: "",
//             lower_bound: null,
//             upper_bound: null,
//             limit: 5,
//           })
//           .then(tableRows => {
//             console.log("TABLE ROWS", tableRows)
//             return contract.vote("ch2", "ch2", 1)
//           })
//       })
//   )
// })
