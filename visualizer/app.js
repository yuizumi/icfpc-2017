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

Operation.prototype.getSplurgeData = function(response){
  let punter = response['punter'];
  let list = [];
  for(let i = 0; i < response['route'].length-1;i++){
    let s = response['route'][i];
    let t = response['route'][i+1];
    if(s>t){
      let tmp = s;
      s = t;
      t = tmp;
    }
    list.push([s,t]);
  }
  return {'splurge':{"p":punter,"r":list}};
}

var Game = function(gameId,selector){
  this._gameId = gameId;
  this._turn = 0;
  this._vis = new Visualizer(selector);
  this._stflag = 0;

  this.startGame = function(){
    this._stflag = 1;
  }

  this.endGame = function(){
    this._stflag = 0;
  }

  this.isGameStart = function(){
    return this._stflag;
  }

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
  this.roleBackTurn = function(){
    if(this._turn >= 0){
        this._turn--;
    }
    return this._turn;
  }
}

Game.prototype.updateVis = function (data) {
  if('map' in data){
    this._vis.createMap(data['map']);
  }else if('claim' in data){
    let tmp = data['claim'];
    this._vis.update(tmp['s'],tmp['t'],tmp['p']);
  }else if('splurge' in data){
    for(dd of data['splurge']['r']){
      this._vis.update(dd[0],dd[1],data['splurge']['p']);
    }
  }
};

Game.prototype.roleBackVis = function (data) {
  if('claim' in data){
    let tmp = data['claim'];
    this._vis.rollback(tmp['s'],tmp['t']);
  }else if('splurge' in data){
    for(dd of data['splurge']['r']){
      this._vis.rollback(dd[0],dd[1]);
    }
  }
};

Game.prototype.finishGame = function () {
  let url = 'log/'+String(this.getGameId())+'/scores.json';
  let jsonlog = $('#jsonlog');
  $.ajax({
    type:"GET",
    dataType:"json",
    url:url
    }
  ).done((function(d) {return function(response,status,jqXHR){
    jsonlog.val(jqXHR['responseText']+' The Game is Over!');
    d._vis.updateScore(response);
  };})(this));
}

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
    d.updateTurn();
    if('claim' in response){
      data = ope.getMoveData(response['claim']);
      d.updateVis(data);
    }else if('splurge' in response){
      data = ope.getSplurgeData(response['splurge']);
      d.updateVis(data);
    }
  };})(this)).fail((function(d) {return function(response,status,error){
    jsonlog.val(error);
    d.finishGame();
    clearInterval(timer);
    //alert("The game is over!! or some error has occur.")
  };})(this));
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

Game.prototype.backOneStep = function(){
  if(game.getTurn() === 0){
    return "stop";
  }
  this.roleBackTurn();
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
    if('claim' in response){
      data = ope.getMoveData(response['claim']);
      d.roleBackVis(data);
    }else if('splurge' in response){
      data = ope.getSplurgeData(response['splurge']);
      d.roleBackVis(data);
    }
  };})(this));

  if(this.getTurn() > 0){
    let url = 'log/'+String(this.getGameId())+'/'+(String(this.getTurn())-1)+'.json';
    $.ajax({
      type:"GET",
      dataType:"json",
      url:url
      }
    ).done(function(response,status,jqXHR){
      jsonlog.val(jqXHR['responseText'])
    });
  }else{
      jsonlog.val('');
  }
}

var game = {};
var timer = {};

$('#play').on('click',function(){
  game = new Game(Number($('#gamenumber').val()),"#main-visualize-cell");
  game.startGame();
  game.initMap();
});

$('#next').on('click',function(){
  game.updateGame();
});

$('#back').on('click',function(){
  game.backOneStep();
});

$('#autoplay').on('click',function(){
  timer = setInterval('game.updateGame();',1000);
});

$('#stopplay').on('click',function(){
  clearInterval(timer);
});

$(window).keydown(function(e){
  if(e.keyCode === 39){
    game.updateGame();
  }
  if(e.keyCode === 37){
    game.backOneStep();
  }
});
