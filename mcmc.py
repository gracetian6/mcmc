import graph

test_graph = graph.DictGraph({'A': [('B',.5),('C',.5)], 'B': [('A',.5),('C',.5)], 'C': [('A',.5),('B',.5)]})

dist = {'A':1/2, 'B':1/4, 'C':1/4}

for x in range(10):
	dist = test_graph.apply(dist)
print(dist)