from __future__ import annotations
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Optional, List
from collections import deque
import statistics
import tkinter as tk
from tkinter import messagebox, ttk

# ------------------------------------------------------------------ #
# LOGICA DEL NEGOCIO Y ESTRUCTURA AVL (Garantiza O(log n))
# ------------------------------------------------------------------ #

class EstadoAcademico(Enum):
    ACTIVO = auto()
    EGRESADO = auto()
    RETIRADO = auto()
    SUSPENDIDO = auto()


@dataclass
class Estudiante:
    codigo: int
    nombre: str
    school: str  # escuela profesional
    ppa: float
    creditos: int
    estado: EstadoAcademico
    semestre_ingreso: str

    def __post_init__(self):
        # Mantenemos la validación original de códigos ajustada al rango de tus nuevos datos
        if not (10000000 <= self.codigo <= 29999999):
            raise ValueError(f"Código inválido: {self.codigo}")
        if not (0.0 <= self.ppa <= 20.0):
            raise ValueError(f"PPA inválido: {self.ppa}")


@dataclass
class NodoBST:
    dato: Estudiante
    izquierdo: Optional["NodoBST"] = field(default=None, repr=False)
    derecho: Optional["NodoBST"] = field(default=None, repr=False)
    altura: int = field(default=0)


class ArbolAcademico:
    def __init__(self):
        self._raiz = None

    def _get_altura(self, nodo: Optional[NodoBST]) -> int:
        return nodo.altura if nodo else -1

    def _get_balance(self, nodo: Optional[NodoBST]) -> int:
        if not nodo:
            return 0
        return self._get_altura(nodo.izquierdo) - self._get_altura(nodo.derecho)

    def _rotar_derecha(self, y: NodoBST) -> NodoBST:
        x = y.izquierdo
        T2 = x.derecho
        x.derecho = y
        y.izquierdo = T2
        y.altura = 1 + max(self._get_altura(y.izquierdo), self._get_altura(y.derecho))
        x.altura = 1 + max(self._get_altura(x.izquierdo), self._get_altura(x.derecho))
        return x

    def _rotar_izquierda(self, x: NodoBST) -> NodoBST:
        y = x.derecho
        T2 = y.izquierdo
        y.izquierdo = x
        x.derecho = T2
        x.altura = 1 + max(self._get_altura(x.izquierdo), self._get_altura(x.derecho))
        y.altura = 1 + max(self._get_altura(y.izquierdo), self._get_altura(y.derecho))
        return y

    def insertar(self, e: Estudiante) -> None:
        self._raiz = self._insertar(self._raiz, e)

    def _insertar(self, nodo: Optional[NodoBST], e: Estudiante) -> NodoBST:
        if nodo is None:
            return NodoBST(e)
        
        if e.codigo < nodo.dato.codigo:
            nodo.izquierdo = self._insertar(nodo.izquierdo, e)
        elif e.codigo > nodo.dato.codigo:
            nodo.derecho = self._insertar(nodo.derecho, e)
        else:
            raise ValueError(f"Código duplicado: {e.codigo}")

        nodo.altura = 1 + max(self._get_altura(nodo.izquierdo), self._get_altura(nodo.derecho))
        balance = self._get_balance(nodo)

        if balance > 1 and e.codigo < nodo.izquierdo.dato.codigo:
            return self._rotar_derecha(nodo)
        if balance < -1 and e.codigo > nodo.derecho.dato.codigo:
            return self._rotar_izquierda(nodo)
        if balance > 1 and e.codigo > nodo.izquierdo.dato.codigo:
            nodo.izquierdo = self._rotar_izquierda(nodo.izquierdo)
            return self._rotar_derecha(nodo)
        if balance < -1 and e.codigo < nodo.derecho.dato.codigo:
            nodo.derecho = self._rotar_derecha(nodo.derecho)
            return self._rotar_izquierda(nodo)

        return nodo

    def buscar(self, codigo: int) -> Optional[Estudiante]:
        nodo = self._buscar(self._raiz, codigo)
        return nodo.dato if nodo else None

    def _buscar(self, nodo: Optional[NodoBST], codigo: int) -> Optional[NodoBST]:
        if nodo is None or nodo.dato.codigo == codigo:
            return nodo
        if codigo < nodo.dato.codigo:
            return self._buscar(nodo.izquierdo, codigo)
        return self._buscar(nodo.derecho, codigo)

    def eliminar(self, codigo: int) -> None:
        if self.buscar(codigo) is None:
            raise KeyError(f"Estudiante con código {codigo} no encontrado")
        self._raiz = self._eliminar(self._raiz, codigo)

    def _eliminar(self, nodo: Optional[NodoBST], codigo: int) -> Optional[NodoBST]:
        if nodo is None:
            return nodo

        if codigo < nodo.dato.codigo:
            nodo.izquierdo = self._eliminar(nodo.izquierdo, codigo)
        elif codigo > nodo.dato.codigo:
            nodo.derecho = self._eliminar(nodo.derecho, codigo)
        else:
            if nodo.izquierdo is None:
                return nodo.derecho
            elif nodo.derecho is None:
                return nodo.izquierdo

            sucesor = self._minimo(nodo.derecho)
            nodo.dato = sucesor.dato
            nodo.derecho = self._eliminar(nodo.derecho, sucesor.dato.codigo)

        if nodo is None:
            return nodo

        nodo.altura = 1 + max(self._get_altura(nodo.izquierdo), self._get_altura(nodo.derecho))
        balance = self._get_balance(nodo)

        if balance > 1 and self._get_balance(nodo.izquierdo) >= 0:
            return self._rotar_derecha(nodo)
        if balance > 1 and self._get_balance(nodo.izquierdo) < 0:
            nodo.izquierdo = self._rotar_izquierda(nodo.izquierdo)
            return self._rotar_derecha(nodo)
        if balance < -1 and self._get_balance(nodo.derecho) <= 0:
            return self._rotar_izquierda(nodo)
        if balance < -1 and self._get_balance(nodo.derecho) > 0:
            nodo.derecho = self._rotar_derecha(nodo.derecho)
            return self._rotar_izquierda(nodo)

        return nodo

    def _minimo(self, nodo: NodoBST) -> NodoBST:
        while nodo.izquierdo:
            nodo = nodo.izquierdo
        return nodo

    def in_order(self) -> List[Estudiante]:
        r: List[Estudiante] = []
        self._in_order(self._raiz, r)
        return r

    def _in_order(self, nodo: Optional[NodoBST], r: list) -> None:
        if nodo:
            self._in_order(nodo.izquierdo, r)
            r.append(nodo.dato)
            self._in_order(nodo.derecho, r)

    def pre_order(self) -> List[Estudiante]:
        r: List[Estudiante] = []
        self._pre_order(self._raiz, r)
        return r

    def _pre_order(self, nodo: Optional[NodoBST], r: list) -> None:
        if nodo:
            r.append(nodo.dato)
            self._pre_order(nodo.izquierdo, r)
            self._pre_order(nodo.derecho, r)

    def post_order(self) -> List[Estudiante]:
        r: List[Estudiante] = []
        self._post_order(self._raiz, r)
        return r

    def _post_order(self, nodo: Optional[NodoBST], r: list) -> None:
        if nodo:
            self._post_order(nodo.izquierdo, r)
            self._post_order(nodo.derecho, r)
            r.append(nodo.dato)

    def bfs(self) -> List[Estudiante]:
        if not self._raiz:
            return []
        r: List[Estudiante] = []
        cola: deque = deque([self._raiz])
        while cola:
            n = cola.popleft()
            r.append(n.dato)
            if n.izquierdo:
                cola.append(n.izquierdo)
            if n.derecho:
                cola.append(n.derecho)
        return r

    def estadisticas(self) -> dict:
        datos = self.in_order()
        if not datos:
            return {}
        ppas = [e.ppa for e in datos]
        
        def contar(n): return 0 if not n else 1 + contar(n.izquierdo) + contar(n.derecho)
        def contar_h(n): 
            if not n: return 0
            if not n.izquierdo and not n.derecho: return 1
            return contar_h(n.izquierdo) + contar_h(n.derecho)

        return {
            "Total Nodos": contar(self._raiz),
            "Total Hojas": contar_h(self._raiz),
            "Altura Árbol": self._get_altura(self._raiz),
            "PPA Promedio": round(statistics.mean(ppas), 2),
            "PPA Máximo": max(ppas),
            "PPA Mínimo": min(ppas),
        }


# ------------------------------------------------------------------ #
# INTERFAZ GRÁFICA (Tkinter Window)
# ------------------------------------------------------------------ #

class AppAcademica:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("Sistema de Registro Académico AVL - O(log n)")
        self.root.geometry("850x650")
        
        self.arbol = ArbolAcademico()
        self._cargar_datos_iniciales()

        # --- PANEL IZQUIERDO: FORMULARIO DE ENTRADA ---
        frame_form = tk.LabelFrame(root, text=" Datos del Estudiante ", padx=10, pady=10)
        frame_form.pack(side=tk.LEFT, fill=tk.Y, padx=10, pady=10)

        # Campos de texto
        tk.Label(frame_form, text="Código (10000000-29999999):").pack(anchor=tk.W)
        self.txt_codigo = tk.Entry(frame_form, width=25)
        self.txt_codigo.pack(pady=2)

        tk.Label(frame_form, text="Nombre Completo:").pack(anchor=tk.W)
        self.txt_nombre = tk.Entry(frame_form, width=25)
        self.txt_nombre.pack(pady=2)

        tk.Label(frame_form, text="Escuela Profesional:").pack(anchor=tk.W)
        self.txt_escuela = tk.Entry(frame_form, width=25)
        self.txt_escuela.pack(pady=2)

        tk.Label(frame_form, text="PPA (0.0 - 20.0):").pack(anchor=tk.W)
        self.txt_ppa = tk.Entry(frame_form, width=25)
        self.txt_ppa.pack(pady=2)

        tk.Label(frame_form, text="Créditos Aprobados:").pack(anchor=tk.W)
        self.txt_creditos = tk.Entry(frame_form, width=25)
        self.txt_creditos.pack(pady=2)

        tk.Label(frame_form, text="Estado Académico:").pack(anchor=tk.W)
        self.cmb_estado = ttk.Combobox(frame_form, values=[e.name for e in EstadoAcademico], width=22, state="readonly")
        self.cmb_estado.current(0)
        self.cmb_estado.pack(pady=2)

        tk.Label(frame_form, text="Semestre Ingreso (ej. 2024-I):").pack(anchor=tk.W)
        self.txt_semestre = tk.Entry(frame_form, width=25)
        self.txt_semestre.pack(pady=2)

        # Botones de Acción Estructurada
        tk.Frame(frame_form, height=15).pack()  # Separador
        
        btn_insertar = tk.Button(frame_form, text="Insertar Estudiante", bg="#4CAF50", fg="white", font=('Helvetica', 10, 'bold'), width=22, command=self.ejecutar_insertar)
        btn_insertar.pack(pady=4)

        btn_buscar = tk.Button(frame_form, text="Buscar por Código", bg="#2196F3", fg="white", font=('Helvetica', 10, 'bold'), width=22, command=self.ejecutar_buscar)
        btn_buscar.pack(pady=4)

        btn_eliminar = tk.Button(frame_form, text="Eliminar por Código", bg="#f44336", fg="white", font=('Helvetica', 10, 'bold'), width=22, command=self.ejecutar_eliminar)
        btn_eliminar.pack(pady=4)

        # --- PANEL DERECHO: CONSOLA Y RECORRIDOS ---
        frame_derecho = tk.Frame(root)
        frame_derecho.pack(side=tk.RIGHT, expand=True, fill=tk.BOTH, padx=10, pady=10)

        # Botones para los Recorridos y Métricas
        frame_botones_rec = tk.Frame(frame_derecho)
        frame_botones_rec.pack(fill=tk.X, pady=5)

        tk.Button(frame_botones_rec, text="In-Order", width=11, command=lambda: self.mostrar_recorrido("In-Order")).pack(side=tk.LEFT, padx=2)
        tk.Button(frame_botones_rec, text="Pre-Order", width=11, command=lambda: self.mostrar_recorrido("Pre-Order")).pack(side=tk.LEFT, padx=2)
        tk.Button(frame_botones_rec, text="Post-Order", width=11, command=lambda: self.mostrar_recorrido("Post-Order")).pack(side=tk.LEFT, padx=2)
        tk.Button(frame_botones_rec, text="BFS (Nivel)", width=11, command=lambda: self.mostrar_recorrido("BFS")).pack(side=tk.LEFT, padx=2)
        tk.Button(frame_botones_rec, text="Estadísticas", bg="#ff9800", fg="white", width=11, command=self.mostrar_estadisticas).pack(side=tk.LEFT, padx=5)

        # Consola de Salida de Texto
        tk.Label(frame_derecho, text="Resultados / Consola del Árbol:").pack(anchor=tk.W)
        self.consola = tk.Text(frame_derecho, wrap=tk.WORD, font=('Courier New', 10))
        self.consola.pack(expand=True, fill=tk.BOTH, pady=5)

        # Mostrar datos iniciales en la consola nada más empezar
        self.mostrar_recorrido("In-Order")

    def _cargar_datos_iniciales(self):
        # Nuevos estudiantes solicitados cargados dinámicamente en el árbol AVL
        estudiantes = [
            Estudiante(20210500, 'Mamani Quispe, Juan', 'Ing. Sistemas', 15.8, 120, EstadoAcademico.ACTIVO, '2021-I'),
            Estudiante(20210300, 'Huanca Apaza, Maria', 'Ing. Civil', 14.2, 110, EstadoAcademico.ACTIVO, '2021-I'),
            Estudiante(20210700, 'Condori Flores, Pedro', 'Medicina', 17.1, 130, EstadoAcademico.ACTIVO, '2021-I'),
            Estudiante(20210100, 'Ticona Lupaca, Rosa', 'Contabilidad', 12.0, 90, EstadoAcademico.SUSPENDIDO, '2021-I'),
            Estudiante(20210400, 'Larico Ccama, Carlos', 'Ing. Sistemas', 16.5, 115, EstadoAcademico.ACTIVO, '2021-I'),
            Estudiante(20210600, 'Cutipa Vargas, Elena', 'Agronomia', 13.7, 100, EstadoAcademico.ACTIVO, '2021-I'),
            Estudiante(20210900, 'Pari Choque, Luis', 'Ing. Sistemas', 18.3, 140, EstadoAcademico.EGRESADO, '2021-I')
        ]
        for est in estudiantes:
            self.arbol.insertar(est)

    def limpiar_consola(self):
        self.consola.delete("1.0", tk.END)

    def escribir_consola(self, texto: str):
        self.consola.insert(tk.END, texto + "\n")

    # --- ENLACES DE LOS BOTONES A LA LÓGICA AVL ---
    
    def ejecutar_insertar(self):
        try:
            est = Estudiante(
                codigo=int(self.txt_codigo.get()),
                nombre=self.txt_nombre.get().strip(),
                school=self.txt_escuela.get().strip(),
                ppa=float(self.txt_ppa.get()),
                creditos=int(self.txt_creditos.get()),
                estado=EstadoAcademico[self.cmb_estado.get()],
                semestre_ingreso=self.txt_semestre.get().strip()
            )
            if not est.nombre or not est.school or not est.semestre_ingreso:
                raise ValueError("Ningún campo de texto puede estar vacío.")

            self.arbol.insertar(est)
            messagebox.showinfo("Éxito", f"Estudiante [{est.codigo}] registrado e insertado en tiempo O(log n).")
            self.mostrar_recorrido("In-Order")
        except ValueError as ex:
            messagebox.showerror("Error de Validación", f"Dato erróneo o campo vacío: {ex}")
        except Exception as ex:
            messagebox.showerror("Error", str(ex))

    def ejecutar_buscar(self):
        try:
            codigo = int(self.txt_codigo.get())
            est = self.arbol.buscar(codigo)
            self.limpiar_consola()
            if est:
                self.escribir_consola("=== ESTUDIANTE ENCONTRADO O(log n) ===")
                self.escribir_consola(f"Código:   {est.codigo}")
                self.escribir_consola(f"Nombre:   {est.nombre}")
                self.escribir_consola(f"Escuela:  {est.school}")
                self.escribir_consola(f"PPA:      {est.ppa}")
                self.escribir_consola(f"Créditos: {est.creditos}")
                self.escribir_consola(f"Estado:   {est.estado.name}")
                self.escribir_consola(f"Semestre: {est.semestre_ingreso}")
            else:
                self.escribir_consola(f"El estudiante con código {codigo} no existe en el árbol.")
        except ValueError:
            messagebox.showerror("Error", "Por favor ingresa un número de código válido en la casilla.")

    def ejecutar_eliminar(self):
        try:
            codigo = int(self.txt_codigo.get())
            self.arbol.eliminar(codigo)
            messagebox.showinfo("Éxito", f"Estudiante con código {codigo} eliminado y árbol rebalanceado en tiempo O(log n).")
            self.mostrar_recorrido("In-Order")
        except KeyError as ex:
            messagebox.showerror("No Encontrado", str(ex))
        except ValueError:
            messagebox.showerror("Error", "Por favor ingresa un número de código válido para eliminar.")

    def mostrar_recorrido(self, tipo: str):
        self.limpiar_consola()
        if tipo == "In-Order":
            lista = self.arbol.in_order()
        elif tipo == "Pre-Order":
            lista = self.arbol.pre_order()
        elif tipo == "Post-Order":
            lista = self.arbol.post_order()
        else:
            lista = self.arbol.bfs()

        self.escribir_consola(f"=== RECORRIDO: {tipo.upper()} ===")
        if not lista:
            self.escribir_consola("El árbol está vacío actualmente.")
            return

        for e in lista:
            self.escribir_consola(f"[{e.codigo}] {e.nombre.ljust(25)} | PPA: {str(e.ppa).ljust(4)} | {e.school}")

    def mostrar_estadisticas(self):
        self.limpiar_consola()
        stats = self.arbol.estadisticas()
        self.escribir_consola("=== MÉTRICAS Y ESTADÍSTICAS DEL ÁRBOL AVL ===")
        if not stats:
            self.escribir_consola("No hay registros cargados.")
            return
        for k, v in stats.items():
            self.escribir_consola(f" - {k}: {v}")


if __name__ == "__main__":
    root = tk.Tk()
    app = AppAcademica(root)
    root.mainloop()
