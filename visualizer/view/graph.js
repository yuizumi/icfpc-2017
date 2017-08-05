const colors = {
    link: [
        '#cccccc',
        '#3CBAC8',
        '#F19181',
        '#68ED63'
    ]
};

const linksToSelectorId = function (selector, src, dst) {
    return selector.replace('#', '') + '-' + src + '-' + dst;
};

const updateEdge = function (rootSelector, srcId, targetId, user) {
    _id = linksToSelectorId(rootSelector, srcId, targetId)
    
    d3
        .selectAll('#'+ _id)
        .style({
            "stroke-width": 5,
            "stroke": colors.link[user]
        });
}

const createGraph = function (rootSelector, data, nodeIndexDic) {
    const width = 500;
    const height = 500;
    const node_radius = 10;

    const edges = _.map(data.edges, function (edge, i) {
        return {
            source: nodeIndexDic[edge[0]],
            target: nodeIndexDic[edge[1]],
            sourceId: edge[0],
            targetId: edge[1],
            user: 0
        }
    });

    const nodes = _.map(data.nodes, function (node, i) {
        return {id: node, isLambda: data.lambdas.indexOf(node) > -1}
    });
    

    const force = d3.layout.force()
        .nodes(nodes)
        .links(edges)
        .size([width, height])
        .linkDistance(50)
        .charge(-200);

    const svg = d3.select(rootSelector).append("svg")
        .attr({width: width, height: height});

    //リンク
    const link = svg.selectAll("line")
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
    const node = svg.selectAll("circle")
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