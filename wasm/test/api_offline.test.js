const assert = require('assert');
const fs = require('fs');
const path = require('path');
const createBibneatModule = require('../lib/node/bibneat.js');

createBibneatModule().then(Module => {
  const db = new Module.BibDB();
  const parser = new Module.Parser(db, false);
  const api = new Module.JsApiHandler(db);

  function loadBibFile(filename) {
    return fs.readFileSync(path.join(__dirname, '../../test/testBibfiles/', filename), 'utf8');
  }

  const expectedArXivIds = ['1003.2133', 'hep-th/9901001', '2106.10522'];

  // Arxiv IDs
  let bib = loadBibFile('arxiv.bib');
  assert(parser.parseString(bib));

  assert.strictEqual(db.numEntries, 3n);
  const idxArXivMap = api.getArXivIds();
  assert.strictEqual(idxArXivMap.size(), 3);

  idxKeys = idxArXivMap.keys();
  idxStart= BigInt(idxKeys.get(0));

  for (var i = 0; i < idxKeys.size(); i++) {
      var idx = idxKeys.get(i);
      assert.strictEqual(idx-idxStart,BigInt(i));
      var arXivId = idxArXivMap.get(idx);
      assert.strictEqual(arXivId,expectedArXivIds[i]);
  }

  console.log('API offline tests passed.');
});
