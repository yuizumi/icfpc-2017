var colors = {
    link: [
        '#cccccc',
        '#3CBAC8',
        '#F19181',
        '#68ED63'
    ]
};

var linksToSelectorId = function (selector, src, dst) {
    return selector.replace('#', '') + '-' + src + '-' + dst;
};

var updateEdge = function (rootSelector, srcId, targetId, user) {
    _id = linksToSelectorId(rootSelector, srcId, targetId)
    
    d3
        .selectAll('#'+ _id)
        .style({
            "stroke-width": 5,
            "stroke": colors.link[user]
        });
}

var createGraph = function (rootSelector, data, nodeIndexDic) {
    var width = 500;
    var height = 500;
    var node_radius = 10;

    var edges = _.map(data.edges, function (edge, i) {
        console.log(edge[0], edge[1]);
        return {
            source: nodeIndexDic[edge[0]],
            target: nodeIndexDic[edge[1]],
            sourceId: edge[0],
            targetId: edge[1],
            user: 0
        }
    });

    var nodes = _.map(data.nodes, function (node, i) {
        return {id: node, isLambda: data.lambdas.indexOf(node) > -1}
    });

    console.log(JSON.stringify(nodes));

    var force = d3.layout.force()
        .nodes(nodes)
        .links(edges)
        .size([width, height])
        .linkDistance(50)
        .charge(-200);

    var svg = d3.select(rootSelector).append("svg")
        .attr({width: width, height: height});

    //リンク
    var link = svg.selectAll("line")
        .data(edges)
        .enter()
        .append("line")
        .style({
            "stroke-width": 2
        })
        .attr("stroke", function (edge) {
            return colors.link[edge.user];
        })
        .attr("id", function (edge) {
            return linksToSelectorId(
                rootSelector, edge.sourceId, edge.targetId
            );
        });


    // ノード
    var node = svg.selectAll("circle")
        .data(nodes)
        .enter()
        .append("circle")
        .attr({
            r: node_radius,
            opacity: 0.5
        })
        .attr("fill", function (d) {
            return d.isLambda ? "red" : "gray";
        })
        .call(force.drag);
    
    force.on("tick", function () {
        link.attr(
            {
                x1: function (d) {
                    return d.source.x;
                },
                y1: function (d) {
                    return d.source.y;
                },
                x2: function (d) {
                    return d.target.x;
                },
                y2: function (d) {
                    return d.target.y;
                }
            });
        node.attr({
            cx: function (d) {
                return d.x;
            },
            cy: function (d) {
                return d.y;
            }
        });

    });
    force.start();
};