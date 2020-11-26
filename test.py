# test.py
from igraph import *
import networkx as nx
from random import choice

# iGraph
g = Graph()
print(g)
g.add_vertices(3)

# networkx
G = nx.Graph()
G.add_edge(1, 2, weight=4)
G.add_edge(2, 4, weight=2)
G.add_edge(1, 3, weight=3)
G.add_edge(3, 4, weight=4)
nx.shortest_path(G, 1, 4, weight='weight')
print(nx.shortest_path(G, 1, 4, weight='weight'))

d = nx.coloring.greedy_color(G, strategy="largest_first")
print(d)
# random vertex
print(choice(list(G.nodes())))
