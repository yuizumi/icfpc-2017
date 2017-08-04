function init_map(){
  let dom = $('#main-visualize-cell');
  $.ajax(
    type:"GET",
    dataType:"json",
    url:""
  ).done(function(response, status, data){
    Visualizer.visualize(dom,data);
  });
  )
}
