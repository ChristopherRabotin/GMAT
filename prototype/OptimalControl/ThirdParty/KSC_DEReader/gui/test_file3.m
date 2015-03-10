function varargout = test_file3(varargin)
% TEST_FILE3 Application M-file for test_file3.fig
%    FIG = TEST_FILE3 launch test_file3 GUI.
%    TEST_FILE3('callback_name', ...) invoke the named callback.

% Last Modified by GUIDE v2.0 03-Jul-2001 14:03:49

if nargin == 0  % LAUNCH GUI

	fig = openfig(mfilename,'reuse');

	% Generate a structure of handles to pass to callbacks, and store it. 
	handles = guihandles(fig);
    handles.bfile=[];
    handles.tfile=[];
	guidata(fig, handles);

	if nargout > 0
		varargout{1} = fig;
	end

elseif ischar(varargin{1}) % INVOKE NAMED SUBFUNCTION OR CALLBACK

	try
		[varargout{1:nargout}] = feval(varargin{:}); % FEVAL switchyard
	catch
		disp(lasterr);
	end

end


%| ABOUT CALLBACKS:
%| GUIDE automatically appends subfunction prototypes to this file, and 
%| sets objects' callback properties to call them through the FEVAL 
%| switchyard above. This comment describes that mechanism.
%|
%| Each callback subfunction declaration has the following form:
%| <SUBFUNCTION_NAME>(H, EVENTDATA, HANDLES, VARARGIN)
%|
%| The subfunction name is composed using the object's Tag and the 
%| callback type separated by '_', e.g. 'slider2_Callback',
%| 'figure1_CloseRequestFcn', 'axis1_ButtondownFcn'.
%|
%| H is the callback object's handle (obtained using GCBO).
%|
%| EVENTDATA is empty, but reserved for future use.
%|
%| HANDLES is a structure containing handles of components in GUI using
%| tags as fieldnames, e.g. handles.figure1, handles.slider2. This
%| structure is created at GUI startup using GUIHANDLES and stored in
%| the figure's application data using GUIDATA. A copy of the structure
%| is passed to each callback.  You can store additional information in
%| this structure at GUI startup, and you can change the structure
%| during callbacks.  Call guidata(h, handles) after changing your
%| copy to replace the stored original so that subsequent callbacks see
%| the updates. Type "help guihandles" and "help guidata" for more
%| information.
%|
%| VARARGIN contains any extra arguments you have passed to the
%| callback. Specify the extra arguments by editing the callback
%| property in the inspector. By default, GUIDE sets the property to:
%| <MFILENAME>('<SUBFUNCTION_NAME>', gcbo, [], guidata(gcbo))
%| Add any extra arguments after the last argument, before the final
%| closing parenthesis.



% --------------------------------------------------------------------
function varargout = pb_cancel_Callback(h, eventdata, handles, varargin)
close(gcbf)

% --------------------------------------------------------------------
function varargout = pb_perform_Callback(h, eventdata, handles, varargin)
[t,e,rpt]=testephg(handles.bfile, handles.tfile);
set(handles.lb_report,'String',rpt)
line(t,e,'Parent',handles.axes1)

% --------------------------------------------------------------------
function varargout = pb_testfile_Callback(h, eventdata, handles, varargin)
[test_file,test_file_path]=uigetfile('*.*','Select an Ephemeris Test File');
if test_file
    handles.tfile=[test_file_path,test_file];
    set(handles.et_testfile,'String',handles.tfile);
    guidata(gcbo,handles)
end

% --------------------------------------------------------------------
function varargout = pb_binary_Callback(h, eventdata, handles, varargin)
[binary_file,binary_file_path]=uigetfile('*.*','Select a Binary Ephemeris File');
if binary_file
    handles.bfile=[binary_file_path,binary_file];
    set(handles.et_binary,'String',handles.bfile);
    guidata(gcbo,handles)
end

% --------------------------------------------------------------------
function varargout = et_binary_Callback(h, eventdata, handles, varargin)
handles.bfile=get(handles.et_binary,'String');
guidata(gcbo,handles)


% --------------------------------------------------------------------
function varargout = et_testfile_Callback(h, eventdata, handles, varargin)
handles.tfile=get(handles.et_testfile,'String');
guidata(gcbo,handles)


% --------------------------------------------------------------------
function varargout = pb_dload_Callback(h, eventdata, handles, varargin)
web ftp://ssd.jpl.nasa.gov/pub/eph/export/test-data -browser
guidata(gcbo,handles)
