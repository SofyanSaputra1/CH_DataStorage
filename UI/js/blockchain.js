const networkConfig = {
  blockchain: "eos",
  host: "127.0.0.1",
  port: 8888,
  chainId: "",
}

document.addEventListener("scatterLoaded", function(extension) {
  const scatter = window.scatter
  const eos = scatter.eos(networkConfig, Eos)
  const identity = scatter
    .getIdentity({ accounts: [{ blockchain: "eos", host: "127.0.0.1", port: 8888 }] })
    .then(identity => {
      const contract = eos.contract("test")
      console.log(identity)
      return contract.then(contract => ({ identity, contract }))
    })
    .then(({ contract, identity }) => {
      const entry = {
        data_hash: "",
        mp: identity.accounts[0].name,
        data_type: "",
        deltaToken: 10,
        timestamp: new Date().toISOString(),
      }

      // contract.crpatdata(entry)
    })
})
