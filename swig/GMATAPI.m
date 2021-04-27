classdef GMATAPI
    methods(Static)
        function val = Construct(varargin)
            val = gmat.gmat.Construct(varargin{:});
            val = GMATAPI.SetClass(val);
        end
        
        function val = Copy(varargin)
            val = gmat.gmat.Copy(varargin{:});
            val = GMATAPI.SetClass(val);
        end
        
        function val = GetObject(arg)
            val = gmat.gmat.GetObject(arg);
            val = GMATAPI.SetClass(val);
        end
        
        function val = GetRuntimeObject(arg)
            val = gmat.gmat.GetRuntimeObject(arg);
            val = GMATAPI.SetClass(val);
        end
        
        function val = SetClass(val)
            typestr = val.GetTypeName();
            if strcmp(typestr, 'ForceModel')
                typestr = 'ODEModel';
            end

            evalstr = strcat('gmat.', char(typestr), '.SetClass(val)');
            try
                val = eval(evalstr);
            catch
            end
        end
    end
end
