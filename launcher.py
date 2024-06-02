import customtkinter as ctk
import os
import webbrowser

ctk.set_appearance_mode("dark")

def launch_game():
    os.system("start \"Starting Game\" \"%appdata%\\JGDPS\\game\\2.2 JGDPS.exe\"")

# callback(url)
def callback(url):
    webbrowser.open_new_tab(url)
    
def linkenters(event):
    webpage.configure(font=("Helveticabold", 15, "underline"))

def linkleaves(event):
    webpage.configure(font=("Helveticabold", 15))

app = ctk.CTk()
app.geometry("900x500")
app.title("JGDPS Launcher")
app.resizable(False, False)

changelog_title = ctk.CTkLabel(app, text="Changelog", font=("Arial", 40))
changelog_title.place(x=20, y=10)

# changelog
onedottwo = ctk.CTkLabel(app, text="v1.2  ●  Switched to Python CustomTkinter, versions in other languages won't be added here")
onedottwo.place(x=20, y=60)

# link
webpage = ctk.CTkLabel(app, text="JGDPS Website", font=("Helveticabold", 15), text_color="#6ABED4")
webpage.place(x=23, y=459)
webpage.bind("<Button-1>", lambda e:
    callback("https://janixgdps.ps.fhgdps.com"))
webpage.bind("<Enter>", linkenters)
webpage.bind("<Leave>", linkleaves)

launch_button = ctk.CTkButton(app, text="Launch Game", command=launch_game, height=60)
launch_button.place(x=730, y=421)

app.mainloop()