var Operation = function(){}

Operation.prototype.getMapData = function (response) {
  let nodes = [];
  let edges = [];
  let lambdas = [];
  response['sites'].forEach(function(node){
    nodes.push(node);
  });
  let comp = function(a,b){
    return a['id']-b['id'];
  }
  nodes.sort(comp);
  response['rivers'].forEach(function(river){
    let s = river['source'];
    let t = river['target'];
    if(s>t){
      let tmp = s;
      s = t;
      t = tmp;
    }
    edges.push([s,t]);
  });
  response['mines'].forEach(function(mine){
    lambdas.push(mine)
  });
  data = {"nodes":nodes,"edges":edges,"lambdas":lambdas};
  return {"map":data};
};

Operation.prototype.getMoveData = function(response){
  let punter = response['punter'];
  let s = response['source'];
  let t = response['target'];
  if(s>t){
    let tmp = s;
    s = t;
    t = tmp;
  }
  return {"claim":{"p":punter,"s":s,"t":t}};
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
  let url = 'log/'+String(this.getGameId())+'/'+String(this.getTurn())+'.json';
  let jsonlog = $('#jsonlog');
  $.ajax({
    type:"GET",
    dataType:"json",
    url:url
    }
  ).done((function(d) {return function(response,status,jqXHR){
    let data = {};
    let ope = new Operation();
    jsonlog.val(jqXHR['responseText']);
    data = ope.getMoveData(response['claim']);
    d.updateTurn();
    d.updateVis(data);
  };})(this)).fail(function(response,status,error){
    jsonlog.val(error);
    alert("The game is over!! or some error has occur.")
  });
};

Game.prototype.initMap = function (){
  let url = 'log/'+String(this.getGameId())+'/map.json';
  let jsonlog = $('#jsonlog');
  $.ajax({
    type:"GET",
    dataType:"json",
    url:url
    }
  ).done((function(d) {return function(response,status,jqXHR){
    let data = {};
    let ope = new Operation();
    jsonlog.val(jqXHR['responseText']);
    data = ope.getMapData(response);
    d.updateVis(data);
  };})(this)).fail(function(response,status,error){
    jsonlog.val(error);
    alert("Cannot download map data!")
  });
}

var game = {};
var timer = {};

$('#play').on('click',function(){
  game = new Game(Number($('#gamenumber').val()),"#main-visualize-cell");
  game.initMap();
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
