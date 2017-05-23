class CommandSolver:
	#コマンドを処理する

	def __init__(self):
		#コンストラクタ
		command = ""
	
	def solveCommand(self, text):
		self.command = text.split()
		print(self.command)
		if(self.command[0] == "exit" and len(self.command) == 1):
			return -1, 0
		else:
			#コマンドによる分岐
			if self.command[0] == "get":
				if self.command[1] == "version":
					return 0, 19
				elif self.command[1] == "angle":
					return 1, 1
			return -1, -1 
