function updateGame(Game){
  console.log(Game);
  let url = 'http://localhost/test/log/'+String(Game.gamenumber)+'/'+String(Game.turn);
  let dom = $('#main-visualize-cell');

  $.ajax({
    type:"GET",
    dataType:"json",
    url:url}
  ).done(function(response){
    if(Game.turn === 0){
      console.log(Game.turn);
      //Visualizer.visualize(dom,data);
    }else{
      console.log(Game.turn);
      //Visualizer.update(dom,data)
    }
    console.log(response);
    Game.turn++;
  });
}

var game = {}

$('#play').on('click',function(){
  game = {gamenumber:Number($('#gamenumber').val()),turn:0}
  updateGame(game);
});

$('#next').on('click',function(){
  updateGame(game);
});

$('#autoplay').on('click',function(){
  updateGame(game);
});

$('#stopplay').on('click',function(){
  updateGame(game);
});
