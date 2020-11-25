# test.py
from igraph import *
import networkx as nx

# iGraph
g = Graph()
print(g)
g.add_vertices(3)

# networkx
G = nx.Graph()
G.add_edge('A', 'B', weight=4)
G.add_edge('B', 'D', weight=2)
G.add_edge('A', 'C', weight=3)
G.add_edge('C', 'D', weight=4)
nx.shortest_path(G, 'A', 'D', weight='weight')
print(nx.shortest_path(G, 'A', 'D', weight='weight'))
