var Operation = function(){
  this.returnData = function(response){
    if('map' in response){
      return this.getmapData(response);
    }else if('claim' in response){
      return this.getMoveData(response);
    }else if('pass' in response){
      return this.getPassData(response);
    }else if('stop' in response){
      return this.getScoreData(response);
    }
  }
}

Operation.prototype.getmapData = function (response) {
  let nodes = [];
  let edges = [];
  let lambdas = [];
  response['map']['sites'].forEach(function(node){
    nodes.push(node['id']);
  });
  let comp = function(a,b){
    return a-b;
  }
  nodes.sort(comp);
  response['map']['rivers'].forEach(function(river){
    let edge = []
    edge.push(river['source']);
    edge.push(river['target']);
    edges.push(edge);
  });
  response['map']['mines'].forEach(function(mine){
    lambdas.push(mine)
  });
  data = {"nodes":nodes,"edges":edges,"lambdas":lambdas};
  return {"map":data};
};

Operation.prototype.getMoveData = function(response){
  let punter = response['claim']['punter'];
  let s = response['claim']['source'];
  let t = response['claim']['target'];
  console.log(s,t);
  if(s>t){
    let tmp = s;
    s = t;
    t = tmp;
  }
  console.log(s,t);
  return {"claim":{"p":punter,"s":s,"t":t}};
}

Operation.prototype.getPassData = function(response){
  return {"pass":"pass"};
}

Operation.prototype.getScoreData = function(response){
  let scores = []
  response['stop']['scores'].forEach(function(score){
    scores.push(score);
  });
  return {"scores":scores};
}

var Game = function(gameId,selector){
  this._gameId = gameId;
  this._turn = 0;
  this._vis = new Visualizer(selector);

  this.getGameId = function(){
    return this._gameId;
  }
  this.getTurn = function(){
    return this._turn;
  }
  this.updateTurn = function(){
    this._turn++;
    return this._turn;
  }
}

Game.prototype.updateVis = function (data) {
  if('map' in data){
    this._vis.createMap(data['map']);
  }else if('claim' in data){
    let tmp = data['claim'];
    this._vis.update(tmp['s'],tmp['t'],tmp['p']);
  }
};

Game.prototype.updateGame = function () {
  let url = 'log/'+String(this.getGameId())+'/'+String(this.getTurn());
  let jsonlog = $('#jsonlog');
  $.ajax({
    type:"GET",
    dataType:"json",
    url:url,
    this:this
    }
  ).done((function(d) {return function(response,status,jqXHR){
    let data = {};
    let ope = new Operation();
    jsonlog.val(jqXHR['responseText']);
    data = ope.returnData(response);
    d.updateTurn();
    d.updateVis(data);
  };})(this)).fail(function(response,status,error){
    jsonlog.val(error);
    alert("The game is over!! or some error has occur.")
  });
};

var game = {};
var timer = {};

$('#play').on('click',function(){
  game = new Game(Number($('#gamenumber').val()),"#main-visualize-cell");
  game.updateGame();
});

$('#next').on('click',function(){
  game.updateGame();
});

$('#autoplay').on('click',function(){
  timer = setInterval('updateGame("#main-visualize-cell",game);',1000);
});

$('#stopplay').on('click',function(){
  clearInterval(timer);
});
