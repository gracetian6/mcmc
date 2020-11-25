class Graph:
	def vertices(self):
		pass

	def neighbors(self, vertex):
		# return a list [(neighbor_1,weight_1),...,(neighbor_k,weight_k)]
		pass

	def degree(self, vertex):
		pass

	def apply(self, vector):
		print(vector)
		new_vector = {}

		for vertex in self.vertices():
			new_vector[vertex] = 0

			for neighbor, weight in self.neighbors(vertex):
				new_vector[vertex] += weight * vector[neighbor]

		return new_vector

class DictGraph(Graph):
	def __init__(self, adjacency_list):
		self.adjacency_list = adjacency_list

	def vertices(self):
		return self.adjacency_list.keys()

	def neighbors(self, vertex):
		return self.adjacency_list[vertex]

	def degree(self, vertex):
		return sum(map(lambda edge: edge[1], self.adjacency_list[vertex]))

def create_random_graph(num_vertices, prob_edge):
	# create Erdos-Renyi graph --- include each edge with prob. prob_edge
	pass

def create_random_regular_graph(num_vertices, degree):
	# create random graph with specified degree
	pass

