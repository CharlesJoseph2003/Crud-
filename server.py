import tkinter as tk
from tkinter import ttk
import socket
import json
import threading
import sys

class ServerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Synth Preset Server")
        
        # Configure main window
        self.root.geometry("800x600")
        
        # Create main frames
        self.left_frame = ttk.Frame(root)
        self.left_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.right_frame = ttk.Frame(root)
        self.right_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Left frame - Server controls and log
        self.status_label = ttk.Label(self.left_frame, text="Server Status: Not Running")
        self.status_label.pack(pady=10)
        
        self.start_button = ttk.Button(self.left_frame, text="Start Server", command=self.start_server)
        self.start_button.pack(pady=5)
        
        ttk.Label(self.left_frame, text="Server Log:").pack(pady=(10,0))
        self.log_text = tk.Text(self.left_frame, height=15, width=40)
        self.log_text.pack(pady=5, fill=tk.BOTH, expand=True)
        
        # Right frame - Preset List
        ttk.Label(self.right_frame, text="Received Presets:").pack(pady=(10,0))
        self.preset_listbox = tk.Listbox(self.right_frame, width=40, height=15)
        self.preset_listbox.pack(pady=5, fill=tk.BOTH, expand=True)
        
        # Preset details
        ttk.Label(self.right_frame, text="Preset Details:").pack(pady=(10,0))
        self.preset_details = tk.Text(self.right_frame, height=10, width=40)
        self.preset_details.pack(pady=5, fill=tk.BOTH, expand=True)
        
        # Server variables
        self.server_socket = None
        self.is_running = False
        self.presets = {}
        
        # Load existing presets from file
        try:
            with open('synth_presets.json', 'r') as f:
                self.presets = json.load(f)
            self.update_preset_list()
        except FileNotFoundError:
            pass
        
        # Bind listbox selection
        self.preset_listbox.bind('<<ListboxSelect>>', self.on_preset_select)
        
        # Start periodic refresh
        self.periodic_refresh()
        
    def on_preset_select(self, event):
        selection = self.preset_listbox.curselection()
        if selection:
            preset_name = self.preset_listbox.get(selection[0])
            if preset_name in self.presets:
                preset = self.presets[preset_name]
                details = f"Name: {preset['name']}\n"
                details += f"Oscillator 1 Freq: {preset['oscillator1_freq']}\n"
                details += f"Oscillator 2 Freq: {preset['oscillator2_freq']}\n"
                details += f"Filter Cutoff: {preset['filter_cutoff']}\n"
                details += f"Resonance: {preset['resonance']}\n"
                details += f"Attack: {preset['attack']}\n"
                details += f"Decay: {preset['decay']}\n"
                details += f"Sustain: {preset['sustain']}\n"
                details += f"Release: {preset['release']}\n"
                
                self.preset_details.delete(1.0, tk.END)
                self.preset_details.insert(tk.END, details)
        
    def log_message(self, message):
        self.log_text.insert(tk.END, f"{message}\n")
        self.log_text.see(tk.END)
        
    def start_server(self):
        if not self.is_running:
            self.server_thread = threading.Thread(target=self.run_server)
            self.server_thread.daemon = True
            self.server_thread.start()
            self.status_label.config(text="Server Status: Running")
            self.start_button.config(state='disabled')
            
    def run_server(self):
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind(('localhost', 12345))
            self.server_socket.listen(1)
            self.is_running = True
            self.log_message("Server started on localhost:12345")
            
            while self.is_running:
                client_socket, address = self.server_socket.accept()
                self.log_message(f"DEBUG: Client connected from {address}")
                
                client_handler = threading.Thread(
                    target=self.handle_client,
                    args=(client_socket, address)
                )
                client_handler.daemon = True
                client_handler.start()
                
        except Exception as e:
            self.log_message(f"Error: {str(e)}")
            if self.server_socket:
                self.server_socket.close()
            self.is_running = False
            self.start_button.config(state='normal')
            
    def handle_client(self, client_socket, address):
        try:
            while True:
                data = client_socket.recv(1024).decode('utf-8')
                if not data:
                    break
                    
                self.log_message(f"DEBUG: Raw data received: {data}")
                
                try:
                    # Reload presets from file
                    with open('synth_presets.json', 'r') as f:
                        self.presets = json.load(f)
                    self.root.after(0, self.update_preset_list)
                    
                    preset = json.loads(data)
                    self.log_message(f"DEBUG: Parsed preset data: {preset}")
                    
                    if 'name' in preset:
                        self.presets[preset['name']] = preset
                        self.log_message(f"DEBUG: Added preset {preset['name']} to presets")
                        self.root.after(0, self.update_preset_list)
                    response = {"status": "success", "message": "Preset received"}
                except json.JSONDecodeError as e:
                    self.log_message(f"DEBUG: JSON decode error: {str(e)}")
                    response = {"status": "error", "message": "Invalid JSON format"}
                except Exception as e:
                    self.log_message(f"DEBUG: Error handling preset: {str(e)}")
                    response = {"status": "error", "message": str(e)}
                
                response_str = json.dumps(response)
                self.log_message(f"DEBUG: Sending response: {response_str}")
                client_socket.send(response_str.encode('utf-8'))
                
        except Exception as e:
            self.log_message(f"Error handling client {address}: {str(e)}")
        finally:
            client_socket.close()
            self.log_message(f"DEBUG: Client {address} disconnected")
            
    def update_preset_list(self):
        self.preset_listbox.delete(0, tk.END)
        for preset_name in self.presets.keys():
            self.preset_listbox.insert(tk.END, preset_name)
            
    def periodic_refresh(self):
        try:
            with open('synth_presets.json', 'r') as f:
                new_presets = json.load(f)
                if new_presets != self.presets:
                    self.presets = new_presets
                    self.update_preset_list()
        except FileNotFoundError:
            pass
        except json.JSONDecodeError:
            pass
        
        # Schedule next refresh
        self.root.after(1000, self.periodic_refresh)  # Check every second

if __name__ == "__main__":
    root = tk.Tk()
    app = ServerGUI(root)
    root.mainloop()
