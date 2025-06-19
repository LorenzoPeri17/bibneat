const createBibneatModule = require('../lib/node/bibneat.js');

createBibneatModule().then(Module => {
  // Test BibEntry
  const entry = new Module.BibEntry("article", "mykey");
  console.log("BibEntry created:", entry.bibKey, entry.bibTypeName);

  // Test BibDB (as shared_ptr)
  const db = new Module.BibDB();
  console.log("BibDB :", db);

  // Test SpecialEntry
  const special = new Module.SpecialEntry("note", "This is a note");
  console.log("SpecialEntry created:", special.bibTypeName, special.body);

  // Test Parser
  const parser = new Module.Parser(db, false);
  console.log("Parser created:", parser);

  // Test FieldFilter
  const filter = new Module.FieldFilter(db);
  console.log("FieldFilter created:", filter);

  // Test ApiCaller
  const api = new Module.ApiCaller(db, 10, 10, false);
  console.log("ApiCaller created:", api.connTimeout, api.totTimeout);

  // Test FieldNormalizer
  const normalizer = new Module.FieldNormalizer(db);
  console.log("FieldNormalizer created:", normalizer);

  // Test Printer
  const printer = new Module.Printer(db);
  console.log("Printer created:", printer);
});
