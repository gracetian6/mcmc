import networkx as nx
from random import choice

class colorGraph:
	def __init__(self, G):
        # graph G, maybe set of colorings C?
        self.G = G

	def randomVertex(self):
        # return random vertex in graph
        return choice(list(G.nodes()))

	def randomColor(self, C):
        # return random color in C
        return choice(list(C))

    def validColor(self, G, v, c, C):
        # check if vertex v with coloring c is valid coloring
        # do this with BFS or DFS?
        pass

    def colorVertex(self, vertex, c):
        # assign vertex color c
        pass

    def sampleColor(self, G, k, t):
        pass
        # graph G (undirected, unweighted)
        # k : # of colors
        # t : # of steps

        # C <- empty coloring
        # for F in range(t):
            # v <- random vertex in G
            # c <- random colors
            # if you take C and then change it
            # by assigning vertex v to have color c
