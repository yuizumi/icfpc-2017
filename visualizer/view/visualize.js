var Visualizer = function (rootSelector) {
    /*
     描画するrootのセレクタを指定してnewで呼び出してください
     */

    this.init = function () {
        this.RootSelector = rootSelector;
        this.MapData = {};
        this.NodeIndexDic = {};
        console.log('selector', rootSelector);
    };

    this.setMapData = function (mapData) {
        this.MapData = JSON.stringify(mapData);
    };

    this.getMapData = function () {
        return JSON.parse(this.MapData);
    };

    // マップの初期化
    this.createMap = function (data) {
        /*
         data: {
         nodes: [1, 2, 3, 5 ...],
         edged: [[1, 2], [1, 5], [2, 3] ...],
         lambdas: [1 ...]
         }
         */
        
        this.NodeIndexDic = {};
        for (var i = 0; i < data.nodes.length; i++) {
            this.NodeIndexDic[data.nodes[i]] = i;
        }
        
        this.setMapData(data);
        createGraph(
            this.RootSelector, 
            this.getMapData(),
            this.NodeIndexDic
        );
        // Test();
        return 0;
    };


    // エッジの更新
    this.update = function (src_node_id, dst_node_id, user_id) {
        console.log(src_node_id + '->' + dst_node_id + ': ' + user_id);
        updateEdge(this.RootSelector, src_node_id, dst_node_id, user_id);
        return 0;
    };


    //オブジェクトのloadを終えたらinitする
    this.init();
};

// テスト
var VisualizeTest = function () {
    var vis = new Visualizer('#main-visualize-cell');

    vis.createMap({
        nodes: [0, 1, 2, 5],
        edges: [
            [0, 5],
            [0, 1],
            [0, 2]
        ],
        lambdas: [1]
    });

    vis.update(0, 1, 1);
};

// VisualizeTest();